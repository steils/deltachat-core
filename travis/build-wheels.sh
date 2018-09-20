#!/bin/bash
set -e -x

# Install a system package required by our library
yum install -y openssl-devel sqlite3-devel zlib-devel libsals-devel bzip2-devel

# Compile wheels
for PYBIN in /opt/python/*/bin; do
    "${PYBIN}/pip" install cffi
    pushd python
    "${PYBIN}/pip" wheel /io/ -w wheelhouse/
    popd
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
