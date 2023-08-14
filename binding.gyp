{
  "targets": [
    {
      "target_name": "camera-settings",
      "sources": [ 
        "src/index.cpp",
        "src/settings_promise_worker.hpp",
        "src/get_settings.hpp",
        "src/set_settings.hpp",
        "src/get_resolution.hpp"
      ],
      "include_dirs": [
        "<!@(node -p \"require('node-addon-api').include\")",
        "include"
      ],
      'dependencies': [
        "<!(node -p \"require('node-addon-api').gyp\")"
      ],
      'defines':['NAPI_DISABLE_CPP_EXCEPTIONS'],
      "conditions": [
          [
            'OS=="win"', {
              "cflags": ["-fno-exceptions"],
              "cflags_cc": ["-fno-exceptions"],
              'msvs_settings': {
                'VCCLCompilerTool': { "ExceptionHandling": 1, 'AdditionalOptions': [ '-std:c++17' ] }
              },
              "libraries": [
                "-lstrmiids"
              ],
              "sources": [
                  "src/win/camera_settings.cpp"
              ]
            }
          ],
          [
            'OS=="linux"', {
              "cflags": ["-fexceptions", "-fPIC"],
              "cflags_cc": ["-fexceptions", "-fPIC"],
              "sources": [
                  "src/linux/camera_settings.cpp"
              ]
            }
          ],
      ]
    }
  ]
}