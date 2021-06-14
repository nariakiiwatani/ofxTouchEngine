# ofxTouchEngine
[TouchEngine](https://github.com/TouchDesigner/TouchEngine-Windows) wrapper for openFrameworks

## Caution
This repository is in very rough and experimental stage.  
Destructive changes may casually occur.

## Supported platforms
Windows only.  
I will support macOS as soon as official TouchEngine for macOS is released.  
[coming soon?](https://github.com/TouchDesigner/TouchEngine-Windows/issues/1)

## Supported features for now

### Parameters(get/set)

- bool
- std::string
- std::vector\<float>
- std::vector\<double>

### Input operators

- ofTexture(TOP)

### Output operators

- std::vector<std::vector\<float>>(CHOP)
- std::vector\<float>(CHOP with single channel or time sliced)
- ofTexture(TOP)

## License

as no license is specified in original repository except source files, I am not able to say anything about this repository.  
Best regards.