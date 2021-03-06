#!/bin/sh

# After EFfects
ae_path="/Applications/Adobe After Effects 2020"
# DuME Repo
dume=$HOME/DEV_SRC/RxOT/DuQF/DuME
# DuAEF Repo
duaef=$HOME/DEV_SRC/RxOT/DuAEF/DuAEF

ae_scripts="$ae_path/Scripts/ScriptUI Panels"
dume="$dume/src-ae"

"$duaef/tools/dev tools/"./dev_install_macos.sh

# DuME required
dume_required="$ae_scripts/dume_required/"
rm -r -f "$dume_required"
rm "$ae_scripts/DuME.jsx"
mkdir "$dume_required"

# icons
mkdir "$dume_required/icons"
for file in "$dume/dume_required/icons/"* ; do
    cp "$file" "$dume_required/icons"
done

for file in "$dume/dume_required/"*.jsxinc ; do
   cp "$file" "$dume_required"
done
cp "$dume/DuME.jsx" "$ae_scripts"