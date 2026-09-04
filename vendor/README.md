# vendor/

`gen3.elf` — the exact compiler every number in this study was produced with.

md5 `1aa4317fcb7adef1b6ad6782d65dcb6b`, built from `Sounio-lang/sounio`
`feat/w1-qd128-transcend` @ `654ba36260`. 2.5 MB, statically linked, Linux
x86-64, no dynamic dependencies.

## Why a binary is committed here

Until it was, the pin named a build that existed on one machine. `RESULTS.md`
gave a reproduction command referencing a compiler nobody else could obtain, and
CI could not check a single number — `tools/verify.sh` had nothing to run
against. Every figure here was reproducible in principle and, by anyone else,
not in practice.

The alternative is checking out the compiler at that commit and rebuilding.
That works and is worth doing — the build is a fixed point, gen2 and gen3 come
out bit-identical, and it takes about seven seconds. It is not sufficient on its
own: it requires the commit to stay reachable, and reproducing a study should
not depend on another repository's branch policy.

So both. The commit is published, and the exact artifact is here.

## Checking it

`tools/verify.sh` reads the pin out of `sio/*.output.txt` and refuses to run
against any other build, so this file cannot silently drift from the captures it
is supposed to match:

```
bash tools/verify.sh vendor/gen3.elf
```

Rebuilding from source should reproduce this md5 exactly. If it does not, the
build is not the fixed point it claims to be, and that is a finding about the
compiler rather than about this directory.
