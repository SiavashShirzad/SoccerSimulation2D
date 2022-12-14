#!/bin/sh

LIBPATH=/usr/local/lib
if [ x"$LIBPATH" != x ]; then
  if [ x"$LD_LIBRARY_PATH" = x ]; then
    LD_LIBRARY_PATH=$LIBPATH
  else
    LD_LIBRARY_PATH=$LIBPATH:$LD_LIBRARY_PATH
  fi
  export LD_LIBRARY_PATH
fi

DIR=`dirname $0`

player="${DIR}/sample_player"
teamname="HELIOS_base"

host="localhost"
port=6000
debug_server_host=""
debug_server_port=""

player_conf="${DIR}/player.conf"
config_dir="${DIR}/formations-dt"

number=0
goalie=""

debugopt=""

offline_logging=""
offline_mode=""
debug_fullstate=""

usage()
{
  (echo "Usage: $0 [options]"
   echo "Possible options are:"
   echo "      --help                   print this"
   echo "  -h, --host HOST              specifies server host (default: localhost)"
   echo "  -p, --port PORT              specifies server port (default: 6000)"
   echo "  -t, --teamname TEAMNAME      specifies team name"
   echo "  -n, --number NUMBER          specifies the reconnect player number"
   echo "  -g, --goalie                 reconnects as a goalie"
   echo "  -f, --formation              specifies the formation directory"
   echo "  --offline-logging            writes offline client log (default: off)"
   echo "  --debug                      writes debug log (default: off)"
   echo "  --debug-server-connect       connects to the debug server (default: off)"
   echo "  --debug-server-host HOST     specifies debug server host (default: localhost)"
   echo "  --debug-server-port PORT     specifies debug server port (default: 6032)"
   echo "  --debug-server-logging       writes debug server log (default: off)"
   echo "  --log-dir DIRECTORY          specifies debug log directory (default: /tmp)"
   echo "  --debug-fullstate            uses a virtual fullstate world model (default: off)"
) 1>&2
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

    -p|--port)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      port=$2
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

    -g|--goalie)
      goalie="-g"
      ;;

    -f|--formation)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      config_dir=$2
      shift 1
      ;;

    --offline-logging)
      debugopt="${debugopt} --offline_logging"
      ;;

    --debug)
      debugopt="${debugopt} --debug"
      ;;


    --log-dir)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      debugopt="${debugopt} --log_dir ${2}"
      shift 1
      ;;

    --debug-server-connect)
      debugopt="${debugopt} --debug_server_connect"
      ;;

    --debug-server-host)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      debug_server_host=$2
      shift 1
      ;;

    --debug-server-port)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      debug_server_port=$2
      shift 1
      ;;

    --debug-server-logging)
      debugopt="${debugopt} --debug_server_logging"
      ;;

    --debug-log-ext)
      if [ $# -lt 2 ]; then
        usage
        exit 1
      fi
      debugopt="${debugopt} --debug_log_ext ${2}"
      shift 1
      ;;

    --debug-fullstate)
      debug_fullstate="--use_fullstate false --debug_fullstate true"
      ;;

    *)
      usage
      exit 1
      ;;
  esac

  shift 1
done

if [ $number -eq 0 ]; then
  echo "ERROR: The reconnect player number is a mandatory option."
  echo ""
  usage
  exit 1
fi

if [ X"${debug_server_host}" = X'' ]; then
  debug_server_host="${host}"
fi

if [ X"${debug_server_port}" = X'' ]; then
  debug_server_port=`expr ${port} + 32`
fi


opt="--player-config ${player_conf} --config_dir ${config_dir}"
opt="${opt} -h ${host} -p ${port} -t ${teamname} -r ${number} ${goalie}"
opt="${opt} ${debug_fullstate}"
opt="${opt} --debug_server_host ${debug_server_host}"
opt="${opt} --debug_server_port ${debug_server_port}"
opt="${opt} ${offline_logging}"
opt="${opt} ${debugopt}"

$player ${opt} &
