#!/bin/sh

export LD_LIBRARY_PATH=/usr/local/lib

DIR=`dirname $0`

player="${DIR}/sample_player"
coach="${DIR}/sample_coach"
trainer="${DIR}/sample_trainer"
teamname="HELIOS_base"
host="localhost"

config="${DIR}/player.conf"
config_dir="${DIR}/formations-dt"

number=11
usecoach="true"

sleepprog=sleep
goaliesleep=1
sleeptime=1

DEBUGOPT=""

usage()
{
  (echo "Usage: $0 [options]"
   echo "Possible options are:"
   echo "      --help                print this"
   echo "  -h, --host HOST           specifies server host"
   echo "  -t, --teamname TEAMNAME   specifies team name"
   echo "  -n, --number NUMBER       specifies the number of players"
   echo "  -c, --with-coach          specifies to run the coach"
   echo "  -C, --without-coach       specifies not to run the coach"
   echo "  --uva                     use UvA type formation"
   echo "  --bpn                     use BPN formation"
   echo "  --debug                   create debug log"
   echo "  --debug-connect           connect to Soccer Viewer"
   echo "  --debug-write             create Soccer Viewer log") 1>&2
}

while [ $# -gt 0 ]
	do
	case $1 in

    --help)
      usage
      exit 0
      ;;

    -h|--host)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      host=$2
      shift 1
      ;;

    -t|--teamname)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      teamname=$2
      shift 1
      ;;

    -n|--number)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      number=$2
      shift 1
      ;;

    -c|--with-coach)
      usecoach="true"
      ;;

    -C|--without-coach)
      usecoach="false"
      ;;

    --uva)
      config_dir="${DIR}/formations-uva"
      ;;

    --bpn)
      config_dir="${DIR}/formations-bpn"
      ;;

    --debug)
      debugopt="${debugopt} --debug"
      ;;

    --debug-connect)
      debugopt="${debugopt} --debug_connect"
      ;;

    --debug-write)
      debugopt="${debugopt} --debug_write"
      ;;

    *)
      usage
      exit 1
      ;;
  esac

  shift 1
done

OPT="-h ${host} -t ${teamname}"
OPT="${OPT} --player-config ${config} --config_dir ${config_dir}"
OPT="${OPT} ${debugopt}"

$player ${OPT} --player_number 11 &
$sleepprog $sleeptime

$trainer -h $host -t $teamname &
