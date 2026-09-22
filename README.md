# streamtools

Small terminal utilities for building live-stream overlays: a live clock, a
"now playing" display driven by a background-music RTMP loop.

These are designed to be run in a terminal that gets captured as a video
source (e.g. an OBS terminal/window capture), so each program clears the
screen and redraws in place.

## Tools

### `clock`

Prints a live-updating date and time, redrawing once per second.

```sh
./clock <TimeZone>
```

- `<TimeZone>` is any value accepted by the `TZ` environment variable
  (e.g. `Asia/Tokyo`, `America/New_York`, `UTC`).

Example:

```sh
./clock Asia/Tokyo
```

### `current-bgm`

Watches a file and reprints its contents every time it changes, debounced to
once per second. Intended to display the "now playing" track name written by
`rtmp-bgm.sh`.

```sh
./current-bgm <filepath>
```

Creates `<filepath>` if it doesn't already exist, then watches it via
`inotify` and redraws on every write.

### `rtmp-bgm.sh`

Loops over local `.mp3`/`.m4a` files in random order and streams each one to
an RTMP endpoint via `ffmpeg`, re-encoding audio to AAC while copying video
(useful for streaming files that already carry a static/video track). Before
each track, it writes the track name to
`$XDG_RUNTIME_DIR/current-bgm.txt` so `current-bgm` can display it.

```sh
export RTMP_BGM="rtmp://your-server/live/stream-key"
./rtmp-bgm.sh
```

- Searches recursively from the current directory for `.mp3`/`.m4a` files.
- After each track, waits up to 5 seconds for input; press `q` to quit,
  or let it time out to continue to the next track.
- Reshuffles the playlist once it reaches the end.

## Building

Requires a C compiler (`cc`/`gcc`) and a Linux system (uses `inotify`).

```sh
make        # builds ./clock and ./current-bgm
make clean  # removes built binaries
```

`rtmp-bgm.sh` additionally requires `bash` and `ffmpeg` on `PATH`.

## Typical setup

1. Start the BGM stream: `RTMP_BGM=rtmp://... ./rtmp-bgm.sh`
2. In a separate terminal (captured by your streaming software):
   `./current-bgm "$XDG_RUNTIME_DIR/current-bgm.txt"`
3. In another captured terminal: `./clock Asia/Tokyo`

## License

MIT — see [LICENSE](LICENSE).
