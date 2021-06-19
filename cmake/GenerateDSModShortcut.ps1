$DS_INSTALL_PATH = $args[0]
$DSMOD_PATH = $args[1]
$BITS_PATH = $args[2]

$CMAKE_BINARY_PATH = (Get-Item .).FullName
$USER_PATH = Join-Path -Path $CMAKE_BINARY_PATH -ChildPath "DSMODUserPath"
$FULL_LNK_PATH = Join-Path -Path $CMAKE_BINARY_PATH -ChildPath "OpenSiege.lnk"

# write-host "DS_INSTALL_PATH: $DS_INSTALL_PATH`nDS_MOD_PATH: $DSMOD_PATH`nBITS_PATH: $BITS_PATH`nCMAKE_BINARY_DIR: $CMAKE_BINARY_PATH`nShortcut Buit: $FULL_LNK_PATH"

$WScriptShell = New-Object -ComObject WScript.Shell

$Shortcut = $WScriptShell.CreateShortCut($FULL_LNK_PATH)
$Shortcut.WorkingDirectory = $DS_INSTALL_PATH
$Shortcut.TargetPath = $DSMOD_PATH
$Shortcut.Arguments = "fullscreen=false bits=$BITS_PATH map=opensiege user_path=$USER_PATH"
$Shortcut.Save()