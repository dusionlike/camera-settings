{
  "targets": [
    {
      "target_name": "camera-settings",
      "sources": [ 
        "src/index.cpp",
        "src/win/camera_settings.cpp"
      ],
      "libraries": [
        "-lstrmiids"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "include"
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      "cflags": ["-fno-exceptions"],
      "cflags_cc": ["-fno-exceptions"],
      'defines':['NAPI_DISABLE_CPP_EXCEPTIONS'],
      "conditions": [
          [
            'OS=="win"', {
              "cflags": ["-fno-exceptions"],
              "cflags_cc": ["-fno-exceptions"],
              'msvs_settings': {
                'VCCLCompilerTool': { "ExceptionHandling": 1, 'AdditionalOptions': [ '-std:c++17' ] }
              },
            }
          ],
          [
            'OS=="linux"', {
              "cflags": ["-fno-exceptions", "-fPIC"],
              "cflags_cc": ["-fno-exceptions", "-fPIC"],
            }
          ],
      ]
    }
  ]
}