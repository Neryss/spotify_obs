# Spotify display for OBS

A minimalist approach to a spotify display for OBS

## Example 

![](https://files.neryss.pw/videos/output_CROP.gif)

## How does it work?

- First of all it takes a snapshot of the Windows Process and finds the Spotify window.
- The window title is, conveniently enough, the artist and name of the song you're currently listening to! No API involved!
- It then write the title to a file.
- Another thread read this and animate it while writing it to another file.

## What does it mean?

- Since it's C++ code, it sadly only works on Windows... For now.
- It's supposed to be efficient¿ I mean, it's ugly C++ but it's fast and won't bother while streaming, that's the purpose.
- Couldn't have I done it in a more portable language? Yes. And I will be working on it if people use it, else I'll keep using it this way for my streams.

#

## Usage
Open Powershell at the folder location (double clicking the exe will make it crash I'm working on it), and use `./spotify_obs.exe`

Add a text source to OBS, select the output file and that's it! You can customize it with font, colors inside OBS.

You don't need to credit me or anything, but you can add a small note on your stream panel if you wish to do so! Don't gatekeep, share it!

## Config file
You will find a [config file](./config_file.cf) which is an example of how you can format things and will follow the following patterns:

```c
output=<"file_path"> //it's the buffer file where the info will be written
display=<"file_path"> //the file where the animation will be written, you will add this one to OBS
limit=<number> // the number of characters allowed before being truncated and animated
animation_speed<seconds> // seconds between each animation step
poll_rate<seconds> // seconds between each buffer file update and Windows snapshot
```

If you duplicate any field, it will take the second value, if you add another field, it will throw an error, if you enter an invalid value it might also.

#

## Debug

You can start the program with `./spotify_obs.exe -debug` if you have any issue with the program. It will print the config values the program got from the config file.

## Issues/Suggestions

If you have any issues with the program, you can contact [me](https://twitter.com/Neryss002) or open an issue on github, with the debug output.
I will be glad to help!

Same goes if you want to suggest something!
#

## What's next? Why does it exist?

I will try to fix things and update this prototype if people are interested, why not port it to other platforms.

As to why does it exist, well I wanted something minimalist with some "computer craft" vibes as my stream HUD, something not complexe and easy to implement. So I created this, and I hope people who are searching for something similar will find this project!