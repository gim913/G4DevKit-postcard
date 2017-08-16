#include "format.h"

#include "memory.h"
#include "defines.h"
#ifdef _MSC_VER
#include <stdarg.h>
#else
#include "stdarg.h"
#endif
#include "string.h"

void formatInit(Format* f) {
}

void _formatStart(Format* f, const char* formatSpec) {
	f->dataBuf[0] = 0;
	f->p = formatSpec;
	f->last = formatSpec;
	f->end = formatSpec + strlen(formatSpec);
	f->pos = 0;
	f->indexPresent = false;
}

/* eat data from bufPtr and place it in output buffer */
size_t eat(Format* f, const char* bufPtr, size_t bufLen) {
	size_t toWrite = Buf_Size - 1 - f->pos;
	if (bufLen < toWrite) {
		toWrite = bufLen;
	}

	memcpy(f->dataBuf + f->pos, bufPtr, toWrite);
	f->pos += toWrite;
	return toWrite;
}

void parseItem(Format* f) {
	int len = 0;
	do {
#define CHECK_END if (p == f->end) { f->p = p; break; }
		
		const char* p = f->p;
		
		f->indexPresent = false;
		f->index = 0;
		f->alignmentPresent = false;
		f->alignmentSign = false;
		f->alignment = 0;
		f->needsProcessing = false;

		while (p < f->end && *(p) != '{') {
			++p;
		}

		len += eat(f, f->last, p - f->last);
		CHECK_END;

		// skip '{'
		p++;
		CHECK_END;

		// check 'escaped' brace
		if (*p == '{') {
			f->last = p;
			f->p = ++p; // skip the '{{'
			continue;
		}

		// calculate index
		if (*p >= '0' && *p <= '9') {
			f->indexPresent = true;
			while (p < f->end && *p >= '0' && *p <= '9') {
				f->index = 10 * f->index + (*p++ - '0');
			}
			CHECK_END;
		}

		// ok check the alignment
		if (*p == ',') {
			f->alignmentPresent = true;
			p++;
			CHECK_END;

			if (*p == '-' || *p == '+') {
				if (*p == '-') f->alignmentSign = true;
				p++;
				CHECK_END;
			}
			if (p < f->end && *p >= '0' && *p <= '9') {
				while (*p >= '0' && *p <= '9') {
					f->alignment = 10 * f->alignment + (*p++ - '0');
				}
			}
			CHECK_END;
		}

		// ok check format string
		const char* formatStart = NULL;
		if (*p == ':') {
			p++;
			CHECK_END;

			formatStart = p;
			while (p < f->end && *p != '}') {
				p++;
			}
		}

		if (*p != '}') {
			len += eat(f, "{badformat}", 11);

		} else {
			if (formatStart) {
				f->currentFormat = formatStart;
				f->currentFormatSize = p - formatStart;
			}
			f->needsProcessing = true;

			//if (lastPass) {
			//	len += eat(f, "{noarg}", 7);
			//}
			p++;
		}

		f->last = p;
		f->p = p;

		CHECK_END;

	} while (0);
#undef f
}

enum { Mode_Number, Mode_Char, Mode_Pointer, Mode_String };
typedef struct {
	int base;
	bool hexUpper;
	bool hasSign;
	size_t maxPrecision;
	int mode;
} FormatSpecifier;

bool parseFormat(Format* format, FormatSpecifier* fs) {
	const char *f = format->currentFormat;
	const char *fEnd = f + format->currentFormatSize;

	fs->base = 10;
	fs->hexUpper = false;
	fs->maxPrecision = 0;
	fs->mode = Mode_Number;
	fs->hasSign = false;

	if (f && f != fEnd) {
		switch (*f) {
		case 'b': case 'B': fs->base = 2; break;
		case 'o': case 'O': fs->base = 8; break;
		case 'n': case 'N': fs->hasSign = true;
			// fallthrough
		case 'd': case 'D': fs->base = 10; break;
		case 'X': fs->hexUpper = true;
			// fallthrough
		case 'x': fs->base = 16; break;

		case 'c': fs->mode = Mode_Char; break;
		case 's': fs->mode = Mode_String; break;
		case 'p': fs->mode = Mode_Pointer; break;

		default:
			eat(format, "{badspec}", 9);
			return false;
		}
		f++;
		if (f != fEnd) {
			while ((*f) >= '0' && (*f) <= '9') {
				fs->maxPrecision *= 10;
				fs->maxPrecision += (*f++) - '0';
			}
			if (f != fEnd) {
				eat(format, "{badspec}", 9);
				return false;
			}
		}
	}

	return true;
}

void intPrinter_print(FormatSpecifier* fs, u32 v, char* buf, size_t bufLen) {
	size_t maxPrecision = fs->maxPrecision;
	bool hexUpper = fs->hexUpper;
	int base = fs->base;

	// skip the last digits if there's no place to print them
	for (; fs->maxPrecision > fs->hasSign && fs->maxPrecision > bufLen; maxPrecision--) {
		v /= base;
	}

	size_t signLen = (fs->hasSign ? 1 : 0);

	// first check if there's any sense to do the loop over the digits
	if (bufLen > signLen) {
		// loops continues down to 'hasSign' in order
		// to leave a space for a '-' if there's such a need
		if (base != 16) {
			for (; fs->maxPrecision > signLen; fs->maxPrecision--) {
				buf[fs->maxPrecision - 1] = '0' + (v % base);
				v /= base;
			}

		} else {
			for (; fs->maxPrecision > signLen; fs->maxPrecision--) {
				int t = (v % base);
				if (t > 9) {
					buf[fs->maxPrecision - 1] = (fs->hexUpper ? 'A' : 'a') + (t - 10);

				} else {
					buf[fs->maxPrecision - 1] = '0' + t;
				}
				v /= base;
			}
		}
	}
	// add sign (if there's a place for that)
	if (fs->hasSign && bufLen) { buf[fs->maxPrecision - 1] = '-'; }
}

size_t intPrinter_getLen(u32 v, int base) {
	size_t l = 0;
	u32 t = v;
	while (t) {
		l++;
		t /= base;
	}
	if (!v) l++;
	return l;
}

size_t fill(Format* f, size_t count) {
	size_t toWrite = Buf_Size - 1 - f->pos;
	toWrite = mmin(count, toWrite);

	memset(f->dataBuf + f->pos, ' ', toWrite);
	f->pos += toWrite;
	return toWrite;
}

void doPrintInt(Format* f, FormatSpecifier* fs, u32 v) {
	if (fs->hasSign && fs->base == 10 && (i32)v < 0) {
		v = -(i32)v;
	}

	// we always want to print whole number
	size_t l = intPrinter_getLen(v, fs->base);
	fs->maxPrecision = mmax(fs->maxPrecision, l);
	if (fs->hasSign) { fs->maxPrecision++; }

	if (f->alignmentPresent && f->alignment) {
		if (f->alignment < fs->maxPrecision) {
			f->alignment = 0;
		}
	}

	f->alignment && !f->alignmentSign && fill(f, f->alignment - fs->maxPrecision);

	size_t toWrite = Buf_Size - 1 - f->pos;
	toWrite = mmin(toWrite, fs->maxPrecision);
	intPrinter_print(fs, v, f->dataBuf + f->pos, toWrite);
	f->pos += toWrite;

	f->alignment && f->alignmentSign && fill(f, f->alignment - fs->maxPrecision);
}

void doPrint(Format* f, FormatSpecifier* fs, u32 v) {
	switch (fs->mode) {
	case Mode_Number: doPrintInt(f, fs, v); break;
	default: break;
	}
}

char* formatFormat(Format* f, const char* formatSpec, ...) {
	va_list l;
	FormatSpecifier fs;
	_formatStart(f, formatSpec);
	va_start(l, formatSpec);
	do {
		parseItem(f);
		if (f->needsProcessing) {
			parseFormat(f, &fs);
			u32 val = va_arg(l, u32);
			doPrint(f, &fs, val);
		}
	} while (f->needsProcessing);
	f->dataBuf[f->pos] = 0;
	va_end(l);
	return f->dataBuf;
}
