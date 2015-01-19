#/bin/sh
echo Results of atlas_builder_test07:
mkdir -p results
atlas-builder data/1.bmp --atlas='results/invert-x.png' --layout='results/invert-x.xatlas' data/2.tga --invert-x && \
atlas-builder data/1.bmp --atlas='results/invert-y.png' --layout='results/invert-y.xatlas' data/2.tga --invert-y && \
atlas-builder data/1.bmp --atlas='results/invert-xy.png' --layout='results/invert-xy.xatlas' data/2.tga --invert-x --invert-y || exit 0
echo 'results/invert-x.xatlas'
cat 'results/invert-x.xatlas'
echo 'results/invert-y.xatlas'
cat 'results/invert-y.xatlas'
echo 'results/invert-xy.xatlas'
cat 'results/invert-xy.xatlas'
rm -rf results/* results
