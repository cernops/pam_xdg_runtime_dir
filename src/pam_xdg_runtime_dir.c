#define PAM_SM_SESSION
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <grp.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <security/pam_appl.h>
#include <security/pam_modules.h>
#include <security/pam_ext.h>
#include <pwd.h>
#include <errno.h>
#include <syslog.h>
#include <string.h>
#include <selinux/selinux.h>
#include <selinux/restorecon.h>


#define DEBUG_LOG_LEVEL LOG_DEBUG


PAM_EXTERN int pam_sm_open_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    const char *pam_user;
    struct passwd *pwd;
    int ret;

    // Check for the debug option
    int debug = 0;
    for (int i = 0; i < argc; ++i) {
        if (strcmp(argv[i], "debug") == 0) {
            debug = 1;
            break;
        }
    }

    if (debug) {
        pam_syslog(pamh, DEBUG_LOG_LEVEL, "Debug mode enabled");
    }

    // Get the PAM_USER (user being authenticated)
    ret = pam_get_user(pamh, &pam_user, NULL);
    if (ret != PAM_SUCCESS) {
        pam_syslog(pamh, LOG_ERR, "Error getting PAM_USER: %s", pam_strerror(pamh, ret));
        return PAM_SESSION_ERR;
    }

    // Get user information using getpwnam
    pwd = getpwnam(pam_user);
    if (pwd == NULL) {
        pam_syslog(pamh, LOG_ERR, "Error getting user information for PAM_USER: %s", pam_user);
        return PAM_SESSION_ERR;
    }

    // Create the destination directory if it doesn't exist
    char dest_directory[128];
    snprintf(dest_directory, sizeof(dest_directory), "/run/user/%d", pwd->pw_uid);

    struct stat st;
    if (stat(dest_directory, &st) != 0) {
        // Directory doesn't exist
        if (mkdir(dest_directory, 0700) != 0) {
            // Error creating the directory
            pam_syslog(pamh, LOG_ERR, "Error creating directory: %s - %s", dest_directory, strerror(errno));
            return PAM_SESSION_ERR;
        }

        // Set the ownership of the directory
        ret = chown(dest_directory, pwd->pw_uid, pwd->pw_gid);
        if (ret != 0) {
            pam_syslog(pamh, LOG_ERR, "Error chowning directory: %s - %s", dest_directory, strerror(errno));
            return PAM_SESSION_ERR;
        }

        selinux_restorecon(dest_directory, SELINUX_RESTORECON_XDEV);

        if (debug) {
            pam_syslog(pamh, DEBUG_LOG_LEVEL, "Destination directory created: %s", dest_directory);
        }
    }

    return PAM_SUCCESS;
}


PAM_EXTERN int pam_sm_close_session(pam_handle_t *pamh, int flags, int argc, const char **argv) {
    // Perform any cleanup if necessary
    return PAM_SUCCESS;
}

