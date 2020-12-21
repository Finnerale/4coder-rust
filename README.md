# 4coder-rust

There are still many open todos, but it is usable already.

This is based on the Odin implementation by Tyler. He also has a [guide] on how to set this up.

Once you have that setup, you can add Rust like this:
```bash
cd custom/languages
git clone https://github.com/Finnerale/4coder-rust rust

# These depend on your setup
cd -
custom/bin/build_lang rust
custom/bin/buildsuper_x64-linux.sh path/to/custom.cpp
```

[guide]: https://github.com/terickson001/4files/blob/aff74bf50925b6b07a3ad1da08f901fb306ff56c/custom/4coder_terickson_language.cpp#L4
