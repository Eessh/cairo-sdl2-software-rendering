workspace("cairo-sdl2-software-rendering")
	configurations({ "Debug", "Release" })
	-- defines { "TTF_USE_HARFBUZZ", "LOG_BOII__COLORED_LOGS", "LOG_BOII__HIGHLIGHT_WARN_ERROR_FATAL_STRINGS" }
	-- flags({ "WinMain" })
	filter("configurations:Debug")
		defines({ "DEBUG" })
		symbols("On")
	filter("configurations:Release")
		defines({ "NDEBUG", "O3" })
		optimize("On")
	filter({})
	targetdir("bin/%{cfg.buildcfg}/")

	-- Rocket
	project("cairo-sdl2-software-rendering")
		filter("configurations:Debug")
			kind("ConsoleApp")
		filter("configurations:Release")
			kind("WindowedApp")
		filter({})
		language("C")
		includedirs({
			"include",
			"log-boii",
			"SDL2-2.26.5/x86_64-w64-mingw32/include/SDL2",
			"cairo-windows-1.17.2/include"
		})
		files({
			"main.c",
			"src/*.c",
			"log-boii/*.c"
		})
		filter({ "system:windows" })
			links({
				"SDL2main",
				"SDL2",
				"cairo",
				"mingw32",
				"comdlg32",
				"ole32",
				"gdi32"
			})
			libdirs({ "SDL2-2.26.5/x86_64-w64-mingw32/lib", "cairo-windows-1.17.2/lib/x64" })
		filter({ "system:linux" })
			links({ "SDL2main", "SDL2", "SDL2_ttf" })
		filter({ "system:macos" })
			links({ "SDL2main", "SDL2", "SDL2_ttf" })
