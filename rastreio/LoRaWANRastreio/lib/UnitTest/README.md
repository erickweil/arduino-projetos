# Unit test mocking

## LittleFS for native tests

Requirements:
- Python3 + toml package
python3 -m venv myenv
source myenv/bin/activate

- Make
- Git
- GCC toolchain

```
cd ./lib
git clone https://github.com/littlefs-project/littlefs.git
cd littlefs
make
rm -rf runners
rm -rf tests
rm -rf benches
```