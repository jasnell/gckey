{
  "targets": [{
    "target_name": "gckey",
    "include_dirs": [
      "<!(node -e \"require('napi-macros')\")"
    ],
    "sources": [
      "./src/gckey.cc"
    ],
    "xcode_settings": {
      "OTHER_CFLAGS": [
        "-O3",
        "-std=c++11",
        "-D_GNU_SOURCE"
      ]
    },
    "cflags": [
      "-O3",
      "-std=c++11",
      "-D_GNU_SOURCE"
    ],
    "conditions": [
      ['OS=="win"', {
        "link_settings": {
          "libraries": [
            "-lws2_32.lib"
          ]
        }
      }]
    ],
  }]
}
