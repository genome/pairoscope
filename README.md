pairoscope
==========
pairoscope generates simple static plots of read pairing information. It is useful for viewing putative SV calls from callers that utilize insert size metrics and read pair orientation.

# Building pairoscope

## Build Dependencies
+ For APT-based systems (Debian, Ubuntu), install the following packages:
```bash
sudo apt-get install build-essential git-core cmake zlib1g-dev libncurses-dev doxygen libcairo-dev libfreetype6-dev
````

## Obtain repository
Clone the repository from github:
```bash
git clone https://github.com/genome/pairoscope.git
```

Beginning with version 0.4.1, you can also just download from the [github releases page](https://github.com/genome/pairoscope/releases). 

## Compile the software
pairoscope does not support in-source builds. So create a subdirectory, enter it, build, and run tests:

```bash
mkdir pairoscope/build
cd pairoscope/build
cmake ../
make -j
```

The binary `pairoscope` can then be found under `pairoscope/build/bin`. If you have administrative rights, then run `sudo make install` to install the tool for all users under `/usr/bin`.

