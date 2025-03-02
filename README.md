# 🏠 BitGyeol (빛결) : An Automated Smart Home System Using Motorized, Light-Aware Technology

## 📖 Overview
This project is initiated as a _case study_ to uncover how **adaptive street and house lights & controlled/automatic doors** work under the hood using a prototype, the proponents used a merged inputs of using a **light-detecting resistor** and **real-time clock module**, and **servo motors** to mimic the opening and closing of doors.

## 🚀 Features
### 🖥️ CLI-Controlled
- The serial monitor serves as the control panel for system interactions.
- Users can execute commands to control various system functions.

### 🔀 Manual/Automatic Control
- In **Automatic Mode**, the system uses **LDRs** and an **RTC** to _determine day/night conditions_, this is done to **prevent false triggers** due to obstructions by **cross-checking** with the RTC before execution.
- In **Manual Mode**, any command that has been executed in the automatic mode, will be preserve and will only be changed if the **manual** commands took the authority, this means that, **any state set by automatic mode** _**is overriden by manual mode**_. This ensures **manual intervention takes highest priority** in case of bugs or unexpected behavior.

### ⏳ Customizable Time and Date
- Our real-time clock employs **UNIX time**.
- These are **configurable** via commands (serial monitor):<br>

  ```
    b HH:MM:SS v YY/MM/DD
  ```
  time and date **respectively**. (make sure that the baud rate is configured to default `9600`)
- More commands available in `BitGyeol/figs/PrimitiveCommands-rev1.1.pdf`

### 🎉 Customizable Greetings
- **Liquid Crystal Display (LCD)** messages can be customized
- You can configure **specific greetings** for certain dates (e.g., 1980/02/13 for your mom's birthday).
- Achieved by **adding conditional statements** to the system.

## 📦 Dependencies
- Libraries for RTC and LCD Modules (provided under `dependencies/x.zip`
- Arduino IDE
### Setup Instructions
<details open>
  <summary>1️⃣ Clone the Repository</summary>
  
```
git clone https://github.com/Brhylle/BitGyeol.git
```

</details>
<details close>
  <summary>2️⃣ Install Required Arduino Libraries</summary>

  The system depends on three Arduino libraries:
- [x] RTC-Master.zip
- [x] DS3231 (RTC)
- [x] LCD Module

  There are **two ways to install** these libraries:
  <details>
    <summary close>
     📌 Way #1: Install via Arduino Library Manager
    </summary>
    <ol>
      <li>Open Arduino IDE.</li>
      <li>Go to Sketch → Include Library → Manage Libraries.</li>
      <li>Search for the file.zip and install it.</li>
      <li>Select file.zip and install it.</li>
    </ol>
  </details>
  
  <details close>
    <summary>
    📌 Way #2: Install Manually from ZIP
    </summary>
    <ol>
      <li>Open Arduino IDE.</li>
      <li>Go to Sketch → Include Library → Add .ZIP Library...</li>
      <li>Select file.zip and install it.</li>
    </ol>
  </details>
</details>

  <details close>
    <summary>
      3️⃣ Compile and Upload the Code
    </summary>
    <ol>
      <li>Connect your Arduino board.</li>
      <li>Open the project’s `.ino` file in Arduino IDE.</li>
      <li>Select the correct Board and Port from Tools → Board & Port.</li>
      <li>Click Upload (🔼).</li>
    </ol>
  </details>


## 🙏 Acknowledgments
Special thanks to Raven Fortin, for fixing LDR's wiring abnormalities, Tuazon, Leomar for handling the documentations, and Magnaye, Ace Crizller for helping me construct the house model!
