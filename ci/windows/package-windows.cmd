call "%~dp0..\ci_includes.generated.cmd"

mkdir package
cd package

git rev-parse --short HEAD > package-version.txt
set /p PackageVersion=<package-version.txt
del package-version.txt

7z a "%PluginName%-%PackageVersion%-Windows.zip" "..\release\*"

iscc ..\installer\installer-Windows.generated.iss /O. /F"%PluginName%-%PackageVersion%-Windows-Installer"
