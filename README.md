# myapp 3 - Desktop Calendar & Chat Application

## 📋 What this README is for
This README is written for someone who does not know how to install or run the app.
It includes every step needed for a new user to:
- download or unzip the project,
- install the required software,
- build the app,
- run the app,
- understand the calendar and images,
- optionally use the chatbot.

---

## 🟢 What this app does

This app is a desktop event planner for macOS that includes:
- a **weekly calendar** view for Monday through Sunday,
- **task scheduling** with start and end times,
- **color-coded task display**,
- an optional **AI chatbot** feature.

This is a desktop application, not a phone app.

---

## 📁 What files should be in the folder

Open the project folder and confirm that it contains these files:
- `main.cpp`
- `calendar.h`
- `calendar_examples.h`
- `imgui/` (folder)
- `stb_image.h`
- `build.sh`
- `package.sh`
- `confetti.png`
- `confetti_real.png`

Optional but useful:
- `chat_history.txt`

If files are missing, the app may not build or display correctly.

---

## 📦 If you downloaded a zip file

1. Open Finder.
2. Find the zip file you downloaded.
3. Double-click the zip file.
4. A folder will appear with the app files.
5. Open that folder.

Now confirm the files listed in the previous section are present.

---

## 🧭 How to open Terminal

Terminal is the program used to type commands.

### Option 1: Use Spotlight
1. Press **Command (⌘) + Space**.
2. Type `Terminal`.
3. Press **Enter**.

### Option 2: Use Finder
1. Open Finder.
2. Go to **Applications**.
3. Open **Utilities**.
4. Double-click **Terminal**.

---

## 📂 How to move Terminal into the app folder

### Option 1: Drag and drop the folder into Terminal
1. Open Terminal.
2. Type `cd ` (include the space).
3. Drag the app folder from Finder into the Terminal window.
4. Press **Enter**.

It should look like this:
```bash
cd "/path/to/myapp 3"
```

### Option 2: Use the folder path from Finder
1. In Finder, select the app folder.
2. Press **Command (⌘) + I**.
3. Copy the path shown under **Where**.
4. In Terminal, type `cd ` and paste the path.
5. Press **Enter**.

Replace `/path/to/myapp 3` with the folder location on your own Mac.

---

## ⚙️ Install required software

This app needs the following on macOS:
- Xcode Command Line Tools
- Homebrew
- GLFW

### 1. Install Xcode Command Line Tools

In Terminal, type:
```bash
xcode-select --install
```

If an installation window appears, click **Install**.

Verify it works by typing:
```bash
clang --version
```

If you see version information, this step is complete.

### 2. Install Homebrew (only if you do not already have it)

Homebrew helps install other software.

In Terminal, type:
```bash
/bin/bash -c "$(curl -fsSL https://raw.githubusercontent.com/Homebrew/install/HEAD/install.sh)"
```

Verify it by typing:
```bash
brew --version
```

### 3. Install GLFW

In Terminal, type:
```bash
brew install glfw
```

This library is required for the app to display windows on macOS.

---

## 🔨 Build the app

1. In Terminal, make sure you are inside the app folder.
2. Type:
```bash
chmod +x build.sh
```
3. Then type:
```bash
./build.sh
```

This will compile the app and create a file named `myapp`.

### What to expect

- If build succeeds, Terminal shows a success message.
- If build fails, Terminal shows an error message.

If you see an error, check the Troubleshooting section below.

---

## ▶️ Run the app

In Terminal, type:
```bash
./myapp
```

A new window should open with the app interface.

If the app does not open, make sure you are still in the same folder and run the command again.

---

## 📅 How the calendar works

The calendar shows one week from **Monday to Sunday**.

- The calendar uses full 24-hour times.
- The start hour can be any number from `0` to `23`.
- The end hour can be any number from `1` to `24`.
- The app sets new tasks to `09:00` to `10:00` by default.
- The calendar is not limited to 9 AM to 8 PM.

### Add a task

1. Open the **Weekly Calendar** tab.
2. Click **+ New Task**.
3. Choose the day of the week.
4. Set the start hour and start minute.
5. Set the end hour and end minute.
6. Enter a title and notes.
7. Click Save.

### Edit a task

- Find the task in the calendar.
- Use the edit controls on the task or in the app interface.

---

## 🖼️ Background images

The app uses two images from the project folder:
- `confetti.png` — login screen background
- `confetti_real.png` — calendar/app page background

### Important note

The code may use hardcoded local paths for these images.
For the app to work for someone else, keep both `confetti.png` and `confetti_real.png` in the project folder.

If the images fail to load, the app still runs. Only the background image will be missing.

---

## 💬 Chatbot (optional)

The Chatbot tab is optional. The app works without it.

If you want chatbot support, install Ollama.

### Install Ollama

In Terminal, type:
```bash
brew install ollama
```

### Download the model

In Terminal, type:
```bash
ollama pull mistral
```

### Start the AI server

In Terminal, type:
```bash
ollama serve
```

Leave this Terminal window open while using the app.
Then open the app and use the **Chatbot** tab.

If the AI server is not running, the chatbot will not respond.

---

## 📦 Package the app

To create a shareable zip file, type:
```bash
chmod +x package.sh
./package.sh
```

This creates `../myapp2.zip` containing the project folder and executable.

---

## 🛠️ Troubleshooting for beginners

### I see “command not found”

- If Terminal says `command not found: clang` or `clang++`, install Xcode tools.
- If it says `command not found: brew`, install Homebrew.
- If it says `command not found: glfw`, install GLFW with `brew install glfw`.

### I see “Permission denied”

If `./build.sh` gives `Permission denied`, run:
```bash
chmod +x build.sh
./build.sh
```

### The app window does not open

- Make sure you are in the project folder.
- Make sure you built the app successfully.
- Run `./myapp` again.

### The chatbot does not respond

Make sure you have run:
```bash
ollama serve
```

Leave that command running in another Terminal window.

### Background image warnings

If the app prints warnings about image loading, it still should run.
Just place the required image files in the app folder or update the code.

---

## ✅ Beginner checklist

- [ ] I unzipped the project folder if I downloaded a zip.
- [ ] I opened Terminal.
- [ ] I changed Terminal into the app folder.
- [ ] I installed Xcode Command Line Tools.
- [ ] I installed Homebrew.
- [ ] I installed GLFW.
- [ ] I ran `chmod +x build.sh`.
- [ ] I ran `./build.sh`.
- [ ] I ran `./myapp`.

---

## 📌 Notes for someone downloading this app

- Replace `/path/to/myapp 3` with your own folder path.
- Do not use a path from another person’s computer.
- Keep `imgui/`, `confetti.png`, and `confetti_real.png` in the folder.
- The chatbot is optional and only works if Ollama is installed and running.

---

## 📁 Important files summary

- `main.cpp` — the main program code
- `calendar.h` — calendar interface and task data
- `calendar_examples.h` — helper data and examples
- `imgui/` — UI library files
- `stb_image.h` — image loader
- `build.sh` — build script
- `package.sh` — zip packaging script

