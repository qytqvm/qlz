# How to publish this as its own GitHub repository

Delete this file from the repository after publishing if you like — it is only for you.

## 1. Create the empty repository
1. Sign in at github.com → **+** (top right) → **New repository**.
2. Name: `qlz`. Owner: your account or a Qytte organisation. Choose **Public**.
3. Do **not** tick "Add a README", ".gitignore" or "licence" (this folder has them).
4. Click **Create repository**.

## 2a. Upload without any tools (easiest)
1. Unzip `qlz-1.0.0.zip` on your computer.
2. On the new repository page click **uploading an existing file**.
3. Open the unzipped `qlz` folder, select **everything inside it** (include the hidden `.github` and `.gitignore` —
   on Mac press Cmd+Shift+. to show hidden files) and drag it in.
4. Commit message: `QLz 1.0.0` → **Commit changes**.
   If `.github` did not upload, use **Add file → Create new file**, name it `.github/workflows/ci.yml`
   and paste the contents of that file.

## 2b. Or with git on the command line
```sh
cd qlz
git init -b main
git add .
git commit -m "QLz 1.0.0"
git remote add origin https://github.com/YOUR-NAME/qlz.git
git push -u origin main
```

## 3. Check the automatic tests
Open the **Actions** tab. The `ci` job builds and tests QLz on Linux and macOS with gcc and clang
(plus a memory-safety run). It should show a green tick within a few minutes.

## 4. Make the release
1. Repository page → **Releases** → **Create a new release**.
2. **Choose a tag** → type `v1.0.0` → **Create new tag**.
3. Title: `QLz 1.0.0`. Description: paste the 1.0.0 section of CHANGELOG.md and the benchmark table from README.md.
4. Attach `include/qlz.h` as a file so people can download the single header directly.
5. **Publish release**.

## 5. Finishing touches (optional)
- About (gear icon on the repo page): description "Tiny fast LZ compressor in one C99 header", website `https://qytte.com`,
  topics `compression`, `lz77`, `c`, `header-only`.
- Link to the repository from the Qytte site and the owner area once it is live.

## What is in here, and what is deliberately not
Included: the QLz header, example, CLI tool, tests, benchmark program, results, format description, MIT licence.
Not included and must stay private: Flash, Turbo, Maximus, Smart Builder, QFin, QStream, the unified decoder,
the website, owner/client areas, white papers and the sample files. QLz contains none of their code.

Before publishing, decide that you are happy for anyone to use QLz commercially for free — that is what MIT allows,
and it cannot be withdrawn for copies people already have.
