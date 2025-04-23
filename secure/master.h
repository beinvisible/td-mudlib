// MorgenGrauen MUDlib
//
// /SECURE/MASTER.C -- master object
//

#include "/secure/config.h"

#include "/sys/userinfo.h"
#include "/sys/shells.h"
#include "/sys/player/base.h"
#include "/sys/moving.h"
#include "/sys/defines.h"
#include "/sys/wizlist.h"
#include "/sys/daemon.h"
#include "/sys/mail.h"
#include "/sys/driver_hook.h"
#include "/sys/functionlist.h"
#include "/sys/properties.h"
#include "/sys/trace.h"
#include "/sys/driver_info.h"
#include "/sys/signals.h"
#include "/sys/interactive_info.h"
#include "/sys/ansi.h"

#include "/secure/wizlevels.h"
#include "/secure/masterflags.h"

#define DEBUG_MSG(x) catch(VOICEMASTER->SendToChannel("Debug", this_object(), SAY, x))

#define DEBUG(x)
#ifndef DEBUG
#define DEBUG(x)	if (funcall(symbol_function('find_player),"debugger"))\
        tell_object(funcall(symbol_function('find_player),"debugger"),x)
#endif

#define log_file(file,str) write_file("/log/"+file, str)
#define NAME(x)            getuid(x)
#define CAP_NAME(x)        capitalize(NAME(x))
#define TP                 efun::this_player()
#define TI                 efun::this_interactive()
#define PO                 efun::previous_object()
#define TO                 efun::this_object()
#define PATH_ARRAY(x) (efun::explode(x, "/")-({"","."}))

#ifndef NOBODY
#define NOBODY "NOBODY"
#endif

// Vom GD aufgerufen:

object      compile_object(string st);
object      connect();
string      creator_file(mixed str);
void        dangling_lfun_closure();
string      *define_include_dirs();
void        destruct_environment_of(object ob);
void        disconnect(object who);
string      *epilog(int eflag);
void        external_master_reload();
void        flag(string str);
string      get_ed_buffer_save_object_name(string file);
string      get_master_uid();
string      get_wiz_name(string file);
mixed       give_uid_to_object(mixed what,object po, int clone);
int         heart_beat_error( object heart_beat, string error, string program,
		              string current_object, int line);
void        inaugurate_master(int arg);
int         log_error(string file,string message);
string      make_path_absolute(string str);
void        move_or_destruct(object what, object to);
void        notify_shutdown();
void        preload(string file);
mixed       prepare_destruct(object ob);
int         privilege_violation(string what, mixed who, mixed where);
int         query_allow_shadow(object ob);
int         query_player_level(string what);
void        receive_imp(string host, string message, int port);
void        remove_player(object victim);
int         retrieve_ed_setup(object who);
void        runtime_error(string err ,string prg, string curobj, int line);
int         save_ed_setup(object who, int code);
void        save_wiz_file();
void        slow_shut_down(int minutes);
int         valid_exec(string name, object ob, object obfrom);
int         valid_query_snoop(object wiz);
mixed       valid_read(string path, string euid, string fun, object obj);
int         valid_snoop(object me, object you);
mixed       valid_write(string path, string euid, string fun, object obj);
static void wiz_decay();


// Userinfo-Handling
void         _cleanup_uinfo();
int          clear_cache();
int          delete_player(string passwd, string real_name);
int          find_userinfo(string user);
mixed        *get_userinfo(string user);
static mixed *get_full_userinfo(string user);
int          get_wiz_level(string user);
string       query_player_object( string name );
int          query_wiz_level(mixed player);
static void  save_userinfo(string user);
int          set_player_object(string user, string objectname);
mixed        *show_cache();
int          update_wiz_level(string user,int lev);
int          update_password(string old,string new);

// Domain-Stuff
int          add_domain_master(string user,string dom);
int          domain_master(string user, string domain);
int          domain_member(string user, string domain);
string       *get_domain_homes(string wiz);
string       *get_domains();
int          remove_domain_master(string user,string dom);
static void  set_domains(string player, mixed *domains);

// Misc
void           BanishName(string name, string reason);
void           UnbanishName(string name);
void           OudateProjectCache(string project);
mixed          QueryBanished(string str);
void           ReloadBanishFile();
void           RestartBeats();
int            advance_wizlevel(string name, int level);
int            allowed_advance_wizlevel(mixed ob);
static void    _cleanup_projects();
static int     create_home(string owner, int level);
static string  *explode_files(string file);
string         *full_path_array(string path, string user);
string         _get_path(string path, string user);
static int     project_access(string user, string project);
mixed          __query_variable(object ob, string var);
void           redo_preload();
int            renew_player_object(mixed who);
void           restart_heart_beat(object heart_beat);
varargs string show_dir(string wo, string user, int flags);
void           start_simul_efun();

// Network
void        FtpAccess(string host, string message, int port);
void        doReadMail(string file);
void        mailread();
string      *mk_rec_list(string str);
void        udp_query(string query,string host,int port);

// Fuer ldmud 3.5.3
#if ! __EFUN_DEFINED__(strlen)
#define strlen(s) \
    sizeof(s)
#endif

#if ! __EFUN_DEFINED__(query_once_interactive)
#include "/sys/object_info.h"
#define query_once_interactive(ob) \
    efun::object_info(ob, OI_ONCE_INTERACTIVE)
#endif

#if ! __EFUN_DEFINED__(seteuid)
#define seteuid(s) \
    (efun::configure_object(efun::this_object(), OC_EUID, s), efun::geteuid() == s)
#endif

#if ! __EFUN_DEFINED__(disable_commands)
#define disable_commands() \
    efun::configure_object(efun::this_object(), OC_COMMANDS_ENABLED, 0)
#endif
