# USER DEFINED
$sse = "E:\Steam\steamapps\common\Skyrim Special Edition"
$outDir = "D:\Skyrim Mod Organizer\mods\True Directional Movement - Modernized Third Person Gameplay"


# GRAB DIRECTORIES
$sseSrc = $sse + "\Data\Scripts\Source"
$usrSrc = (Get-Location)
$papyrus = $sse + "\Papyrus Compiler\PapyrusCompiler.exe"
$flags = $sseSrc + "\TESV_Papyrus_Flags.flg"


# COMPILE PAPYRUS SCRIPTS
& "$papyrus" "$usrSrc\..\Scripts\Source" -f="$flags" -i="$sseSrc;$usrSrc\..\Scripts\Source" -o="$outDir\Scripts" -a
