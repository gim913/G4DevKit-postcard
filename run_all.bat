set RootDir=f:\projects\RUIFIG\ruifig-g4devkit
pushd %RootDir%\DevKit\bin\
start HostingServer.exe
start "" "%RootDir%\DevKit\bin\APCPUSim.exe" workspace="f:\projects\RUIFIG\postcard\postcard.apcpuwsp" servermanager=127.0.0.1:28000 opensimulator %*
