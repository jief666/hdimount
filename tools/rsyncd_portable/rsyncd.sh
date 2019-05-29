#!/bin/bash
SOURCE="${BASH_SOURCE[0]}"
while [ -h "$SOURCE" ]; do # resolve $SOURCE until the file is no longer a symlink
  TARGET="$(readlink "$SOURCE")"
  if [[ $TARGET == /* ]]; then
    #echo "SOURCE '$SOURCE' is an absolute symlink to '$TARGET'"
    SOURCE="$TARGET"
  else
    DIR="$( dirname "$SOURCE" )"
    #echo "SOURCE '$SOURCE' is a relative symlink to '$TARGET' (relative to '$DIR')"
    SOURCE="$DIR/$TARGET" # if $SOURCE was a relative symlink, we need to resolve it relative to the path where the symlink file was located
  fi
done
#echo "SOURCE is '$SOURCE'"
RDIR="$( dirname "$SOURCE" )"
DIR="$( cd -P "$( dirname "$SOURCE" )" >/dev/null 2>&1 && pwd )"
if [ "$DIR" != "$RDIR" ]; then
  #echo "DIR '$RDIR' resolves to '$DIR'"
  :
fi
SCRIPT_DIR="$DIR"

set -x

daemon=1
 
while getopts ":f" opt; do
  case $opt in
    f)
      daemon=0
      ;;
    \?)
      echo "Invalid option: -$OPTARG" >&2
      ;;
  esac
done
shift $((OPTIND-1))




cd "$SCRIPT_DIR"

#LOGFILE=$SCRIPT_DIR/rsyncd.log
LOGFILE=/dev/stdout

sed "s|log file = LOGFILE|log file = $LOGFILE|" <rsyncd.conf.template | \
  sed "s|path = PATH|path = $SCRIPT_DIR/../..|" |
  sed "s|pid file = PIDFILE|pid file = $SCRIPT_DIR/rsyncd.pid|" |
  sed "s|lock file = LOCKFILE|lock file = $SCRIPT_DIR/rsyncd.lock|"  >rsyncd.conf 

rm -f rsyncd.log
rm -f rsyncd.pid

if [ $daemon -eq 1 ]
then
    "$SCRIPT_DIR"/rsync --daemon -v --config="$SCRIPT_DIR"/rsyncd.conf
else
    "$SCRIPT_DIR"/rsync --daemon --no-detach -v --config="$SCRIPT_DIR"/rsyncd.conf
fi

