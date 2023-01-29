# Utility functions
show_help () {
	echo ""
	echo "|===============================|  Help Guide  |===============================|"
	echo "| > Input flags:                                                               |"
	echo "| -c | --copy: The script execution will create a copy of the target.          |"
	echo "| -l | --link: The script execution will create a symbolic link of the target. |"
	echo "| ---------------------------------------------------------------------------- |"
	echo "| > Notes:                                                                     |"
	echo "| If no flag is specified, the script will do nothing.                         |"
	echo "| If both flags are specified, the script will do nothing.                     |"
	echo "|===========================|  End of  Help Guide  |===========================|"
	echo ""
}

# Input flags
while test $# -gt 0; do
	case "$1" in
	-c|--copy) 	FLAG_COPY="TRUE" ;;
	-l|--link) 	FLAG_LINK="TRUE" ;;
	*)			break ;;
	esac
	shift
done

# Checks if no Flag was given
if [ -z "${FLAG_COPY}" ] && [ -z "${FLAG_LINK}" ]; then
	echo "> ERROR: No flag specified."
	show_help
	exit 1
fi

# Checks if both Flags were given
if [ -n "${FLAG_COPY}" ] && [ -n "${FLAG_LINK}" ]; then
	echo "> ERROR: Both flags specified."
	show_help
	exit 1
fi

# Needed paths
origin_path="$(dirname "$(realpath -s "$0")")"
target_path="$(realpath -s "${origin_path}/..")"

# Executes the COPY
if [ -n "${FLAG_COPY}" ]; then
	cp -r "${origin_path}/vscode" "${target_path}/.vscode" || exit 1
	echo "> STATUS: Successfully copied .vscode folder."
fi

# Executes the LINK
if [ -n "${FLAG_LINK}" ]; then
	ln -snf "${origin_path}/vscode" "${target_path}/.vscode" || exit 1
	echo "> STATUS: Successfully linked .vscode folder."
fi

exit 0
