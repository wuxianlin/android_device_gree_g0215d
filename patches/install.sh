echo $1
rootdirectory="$PWD"
# ---------------------------------

dirs="system/sepolicy device/qcom/sepolicy frameworks/base"

for dir in $dirs ; do
	cd $rootdirectory
	cd $dir
	echo "Applying $dir patches..."
	#git apply $rootdirectory/device/gree/g0215d/patches/$dir/*.patch
	git am $rootdirectory/device/gree/g0215d/patches/$dir/*.patch
	echo " "
done

# -----------------------------------
echo "Changing to build directory..."
cd $rootdirectory
