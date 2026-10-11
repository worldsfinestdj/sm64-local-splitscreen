# Publish This Project to GitHub

This folder is already a local Git repository and is prepared for publishing.
The ROM, extracted assets, compiled executable, saves, controller settings, and
shortcuts are ignored.

## Create the GitHub repository

1. Sign in at [GitHub](https://github.com).
2. Select **New repository**.
3. Name it `sm64-local-splitscreen`.
4. Add the description: `Two-player local split-screen edition of SM64CoopDX.`
5. Choose **Public** if you want others to view it, or **Private** if it is for
   your own backup.
6. Leave **Add a README**, **.gitignore**, and **license** unchecked. Those are
   already in this folder.
7. Select **Create repository**.

## Connect and upload

From a terminal opened in this project folder, replace `YOUR-ACCOUNT` below
with your GitHub account name, then run:

```text
git remote add origin https://github.com/YOUR-ACCOUNT/sm64-local-splitscreen.git
git push -u origin main
```

GitHub may ask you to sign in in the browser. After the upload finishes, open
the repository page. Its front page will show the features, patch notes, and
credits automatically.

## Before publishing a later update

```text
git add .
git commit -m "Describe the update"
git push
```

Always check `git status` before uploading. If it lists a ROM, an `.exe`, a
save, or personal controller settings, stop and ask for help before pushing.
