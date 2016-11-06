echo $1
rootdirectory="$PWD"
# ---------------------------------

dirs="system/sepolicy device/qcom/sepolicy frameworks/base"

for dir in $dirs ; do
	cd $rootdirectory
	#cd $dir
	echo "Reverting $dir patches..."
	#git apply --reverse $rootdirectory/device/gree/g0215d/patches/$dir/*.patch
	repo sync $dir
	echo " "
done

# -----------------------------------
echo "Changing to build directory..."
cd $rootdirectory
