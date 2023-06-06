# Crypto libraries for the Tillitis TKey
Only contains SHA1 & HMAC-SHA1 – for now. 

- Common RV32 arithmetic functions: `libarithmetic`.
  - replace `libgcc`, extracted from gcc 12
- SHA1 & HMAC-SHA1: `libsha`.
  - extracted from the [cryb-to](https://github.com/cryb-to/cryb-to) project

## Building
We advise to build the libraries with our OCI image 
`ghcr.io/nowitis/tkey-deviceapp-builder`. 

Then:
```
make podman
```

Please see the
[README on the tkey-libs repo](https://github.com/tillitis/tkey-libs)
for detailed information on the currently supported build and development
environment.

## Licenses and SPDX tags
Unless otherwise noted, the project sources are licensed under the
terms and conditions of the "GNU General Public License v2.0 only".
See [LICENSE](LICENSE) for the full GPLv2-only license text.

External source code we have imported are isolated in their own
directories. Each file is prefixed with its licensing information.

The project uses single-line references to Unique License Identifiers
as defined by the Linux Foundation's [SPDX project](https://spdx.org/)
on its own source files, but not necessarily imported files. The line
in each individual source file identifies the license applicable to
that file.

The current set of valid, predefined SPDX identifiers can be found on
the SPDX License List at:

https://spdx.org/licenses/
