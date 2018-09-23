#!/bin/bash
set -e -x

# Install a system package required by our library
yum install -y openssl-devel bzip2-devel

# Install meson and ninja
/opt/python/cp37-cp37m/bin/pip install meson
pushd /usr/bin && ln -s /opt/_internal/cpython-3.7.0/bin/meson && popd
# curl -O https://github.com/ninja-build/ninja/releases/download/v1.8.2/ninja-linux.zip
# unzip ninja-linux.zip
# mv ninja /usr/bin/ninja
cp /io/ninja /usr/bin/ninja

# Build the library
meson -Dmonolith=true /builddir /io
ninja -C /builddir

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" install cffi
    "${PYBIN}/pip" wheel /io/python -w wheelhouse/
done

# Bundle external shared libraries into the wheels
for whl in wheelhouse/*.whl; do
    auditwheel repair "$whl" -w /io/wheelhouse/
done

# Install packages and test
for PYBIN in /opt/python/*/bin/; do
    "${PYBIN}/pip" install deltachat --no-index -f /io/wheelhouse
    #(cd "$HOME"; "${PYBIN}/nosetests" pymanylinuxdemo)
done
