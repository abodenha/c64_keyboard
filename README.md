# The C64 USB Keyboard
## What is it?
This is a PCB, CircuitPython code, and keyboard layout that uses [Adafruit's ItsyBitsy M4 Express](https://www.adafruit.com/product/3800) turn a C64 into a USB keyboard you can use on a modern computer.

Right now, you'll need to source all of the parts yourself. It requires basic soldering skills but it's all through-hole parts. If there's enough interest, I might offer a kit or fully assembled unit for sale.

## But WHY?
I've been into collecting and repairing 80s home computers lately and one thing I often chuckle at is that I often eBay see listings for "Commodore 64 Keyboard". Some people just can't get their heads around the idea that computer and keyboard in a single case and connected to an external display was once a common format. They see a plastic case with buttons and their brain just says "Oh, that must just be the keyboard part."

This project was born when my wife said she really liked the feel of typing on a C64 and that it would be fun if she could use it to type on her modern computer.
Mostly, this gave me a chance to learn how to make PCBs with Fusion 360 and to get familiar with CircuitPython. Beyond that, it was just a fun hack.

## Assembly & Setup
### Parts needed
* [ItsyBitsy M4 Express](https://www.adafruit.com/product/3800)
* Both [male](https://www.adafruit.com/product/4173) and [female](https://www.adafruit.com/product/4174) headers for the ItsyBitsy M4
* Pin headers that'll fit the C64 keyboard cable and LED connector. I used [these](https://www.digikey.com/en/products/detail/molex/0022284360/313821).
* The PCB. You can grab the files here and make your own, order from my PCBWay listing, or order my extras from FRKNetwork
* A 150ohm through-hole resistor
* A C64 case, keyboard, and power LED.

If you don't care about having the power LED light up, you can leave out the resistor and pins on the LED connector.

### Put together the PCB and setup the ItsyBitsy M4 Express

#### Set up the ItsyBitsy M4
First follow [Adafruit's instructions](https://circuitpython.org/board/itsybitsy_m4_express/) to install CircuitPython 8 (7 should work but I haven't tested it.)

When you connect the ItsyBitsy M4 to your computer you should see a removable drive called CIRCUITPY.
1. Install the HID library by copying [Adafruit's HID project bundle](https://learn.adafruit.com/circuitpython-essentials/circuitpython-hid-keyboard-and-mouse) onto CIRCUITPY.
2. Copy boot.py and code.py onto CIRCUITPY

The board should reboot and the CIRCUITPY drive should go away. If you need to get it back, you'll need to boot the board into safe mode (ask Google how). Once everything is assembled, you can get CIRCUITPY to come back by plugging in the board while holding down the Restore key on the keyboard.

#### Assemble the PCB
1. Install the female headers for the ItsyBitsy M4 onto the front of the PCB. You might need to carefully trim the plastic to make them fit. Solder from the back.
2. Push the male pins for the ItsyBitsy M4 loosely into the female headers. Put the ItsyBitsy on top to get everything fully aligned and solder a few pins to hold the male header in place. If you don't line up the pins at this point, they'll be a pain to fit together later (ask me how I know).
3. Pull the ItsyBitsy M4 and male pins out of the PCB and finish soldering the rest of the male pins.
4. Solder the keyboard pins onto the front of the PCB. Pin 2 is removed to match the key in the C64 keyboard connector. You'll either need a strip of 18 pins and a single loose pin, or a strip of 20 with pin 2 removed.
5. If you want to have the power LED lit, attach a 3 pin strip to the LED header and put a 150 ohm resistor in R1.
6. Push the ItsyBitsy M4 fully into the header on the PCB.
7. Mount the board into a C64 case however you like. I used mounting tape to mount it just inside the opening for the cartridge port, but do whatever works for you. Connect the keyboard connector, LED, and USB cable.

#### Optional: Set up the keyboard layout
I set up the scan codes coming from the keyboard to roughly map to what would normally be in each location. That means key presses won't always map to what's on each key's silkscreen. Shift+2, for example, will give @ rather than ". I'd initially planned on mapping it all in the microcontroller, but things quickly got very weird with modifier keys. I could work around it, but each hack created problems that required another hack.
Running the keyboard with a standard EN-US keyboard map works ok and might actually be more usable in some ways (muscle memory). If you want to get as close as possible to a TRUE C64 typing experience though, you'll want to install the keyboard layout.
Right now, I only have a Windows layout created. I'll try to create Linux and Mac layouts later.
To install for Windows:
1. Grab us_c64.klc from the layouts folder in the C64_KEYBOARD repo.
2. Install [Microsoft Keyboard Layout Creator](https://www.microsoft.com/en-us/download/details.aspx?id=102134) (You'll need to install .Net 3.5 first)
3. Open us_c64.klc in the Keyboard Layout Creator.
4. Hit Project > Build DLL and Setup Package
5. Run the resulting setup.exe (click through the scary popups)

You should now have a new keyboard layout in your taskbar. If you select it, the keys should behave much more in line with the silkscreens.

## Using the keyboard
Keyboard layouts have evolved a LOT since the Commodore 64. There are a lot of keys that are essential for using a modern computer that are missing on the C64 keyboard. For some of those, I just repurposed keys as follows:
* Run stop is Alt
* The left arrow in the upper left I made the Esc key
* Inst Del is Backspace

There are also just some keys are too different from modern keys, and it's not clear what to do with them:
* CRSR down and right send down and right arrow keys but you can't get to the up or left arrows just by holding shift (since shift+arrow means to select on modern systems)
* Shift lock is NOT the same as a modern caps lock. It is electrically the same as just holding the left shift down.
* F1 F3 F5 and F7 work as you'd expect, but there's no way to get to the even numbered keys since we need to be able to type shift+F1, etc as their own thing.

So how do we type F2? Or get somewhat usable arrow keys?

The Restore key is special. It has its own pin in the keyboard connector that's directly connected to ground when the key is pressed. That means I can read what's happening with it completely separately from the rest of the keys. That makes it perfect for acting as an extra modifier key separate from the ones that need to be reported to the OS (shift, alt, control, meta). If you hold down restore and do something else, it changes what happens. Specifically:

* Holding restore while the ItsyBitsy M4 is booting up puts it into development mode. That will cause the CIRCUITPY drive to appear again and you can modify the setup. To get back to normal mode, just restart it again (unplug and replug) without the restore key held.
* Restore + W A S or D allows those keys to be used as arrow keys.
* Restore + the CRSR keys will cause them to send up or left
* Restore + a function key sends the the next key. So restore+F1 sends F2, etc.
* Restore + HOME sends END.

## How does it work
The ItsyBitsy M4 is the smallest board I found that had enough of the right sorts of inputs and outputs (digital GPIO) I needed to do what I wanted to do. It was also easily available and cheap and runs CircuitPython which is pretty easy to develop for. 

When the board starts up it runs boot.py (which disables the CIRCUITPY drive and a few other things we don't need most of the time). Once that finishes, it starts code.py which does the actual work of scanning the keys and sending key info over USB.

I found an [excellent deep dive on how the keyboard actually works](https://c64os.com/post/howthekeyboardworks) which helped me figure out how to scan it. Basically, each key (except restore) sits in a particular row and column. There's a pin for each row and for each column on the keyboard connector. When you press a key it connects the pins together for the row and column containing that key. If I push the A key, the pin for row 2 gets connected to the pin for column 1.

To figure out which keys are down, I go through each column one at a time. I set them up as outputs with the first one set to ground and the rest to 3.3v. Then I set up the rows as inputs and set it so that if they're not connected to anything it's the same as being connected to 3.3v (pull up). Then I look at all of the rows. If any of them read as being connected to ground that means that row is connected to the first column, which means the corresponding key is pressed. If I do the same thing for each of the other columns and do it over and over really fast, I can quickly detect each key being pressed.

The USB interface works differently. It's event driven and is looking for scan codes. When code.py detects that the key in row 2, column 1 is pushed it looks up what that means in a table and tells USB "Hey! Key 4 was just pressed."

Code.py also needs to keep track of which keys it has seen so that it can tell USB when they're released. Otherwise USB just thinks you've kept the key down forever. That gets a bit more complicated, because we need to "debounce" the key presses.

When you push a key on the keyboard, it's pretty likely to bounce slightly. If I'm reading the keys really fast that bounce looks like the user released the key and then pressed it again. That gets really annoying when typing because you end up with a lot of double or triple typed letters.

The fix is that when a key is pushed, code.py records not only that the key was pushed, but WHEN 

## What's next
This first release is pretty clunky TBH. I'm counting on my wife to use it for a while to tell me what works and what needs tweaking. I'll almost certainly be adding more keys accessible via restore.

I already know that the board layout needs a bit of tweaking.
* Pads are really close to the edge of the board. I want to move them back a bit.
* The pads and holes for the keyboard header are a bit small. If you use pins big enough to properly fit the C64 keyboard connector they're hard to get into the holes on the PCB.
* I need to add a trace on the LED header so that the LED will work in either orientation.

## FAQ
Q: Can I use a different microcontroller?  
A: Maybe? There are a few different boards mentioned on Adafruit's site that supposedly have the same footprint as the ItsyBitsy M4. If the pinout lines up sensibly you should just be able to tewak code.py a little to make it work, but you're on your own.

Q: Can you create a keyboard layout for (insert your favorite OS here)?  
A: I've got the Windows one working. I haven't taken a look at what'll work with common GNU/Linux distros yet, but I plan to. ChromeOS MIGHT work with that in dev mode but I know there's no way to do it in verified mode. I don't have easy access to a Mac, so that'll be harder. Beyond that, probably not. If you want to hack it together and send it to me, I'll probably be willing to add it.

Q: I really want it to do (x). Can you add it?
A: I'm open to suggestions, but probably not. If there's something you really want to do, just edit code.py yourself.

Q: What about PETSCII?  
A: That would be interesting. It looks like it's possible to represent PETSCII in Unicode, but you'd have to use a font that included those glyphs. It would also require an alternate code.py and layout to make it work that would collide with the mods I made to make the thing minimally usable as a modern keyboard.
