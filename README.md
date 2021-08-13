# kobject
A tool to pull C++ object names from kernel memory

## Compile

1. Have an iOS sdk in `/var/sdks/iPhoneOS.sdk`
2. Run `./build.sh`

## Usage
Supply a kernel virtual memory address as the first argument. If there's a C++ object at this address, the name will be returned.
```
iPod-touch:/var/mobile root# kobject 0xfffffff077837a00
IOMobileFramebufferUserClient
iPod-touch:/var/mobile root#
```

If there is not, the device will either panic, return a weird non-ASCII string, or tell you that there is no object here.

Implementation is a bit hacky, lots of room for improvement. Just someting I threw together a couple months ago.

Billy Ellis (@bellis1000)
