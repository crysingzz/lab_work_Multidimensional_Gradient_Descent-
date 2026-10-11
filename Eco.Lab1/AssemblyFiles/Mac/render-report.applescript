on run arguments
    set inputPath to item 1 of arguments
    set outputPath to item 2 of arguments
    tell application "Pages"
        set reportDocument to open (POSIX file inputPath)
        delay 2
        export reportDocument to (POSIX file outputPath) as PDF
        close reportDocument saving no
    end tell
end run
