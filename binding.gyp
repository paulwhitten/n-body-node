{
    "targets": [{
        "target_name": "nBody",
        "sources": [
            "nBodyAddon.cc",
            "nBody.cc"
        ],
        "include_dirs": [
            "<!(node -e \"require('nan')\")"
        ]
    }]
}