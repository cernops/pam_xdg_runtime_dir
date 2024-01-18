# pam_xdg_runtime_dir

Pam module to create `/run/user/<uid>` directory.

It is useful sometimes to create this directory before pam_systemd and systemd-logind create it.

Enable pam module with 

```
session optional pam_xdg_runtime_dir.so debug
```

added to the pam_stack.

## Example Usecase

Enable this pam module in `/etc/pam.d/sshd` before `pam_systemd` to create the directory and populate it with a
kerberos credential. See the  `pam_krb5_cc_move` pam module.

## Links

* https://github.com/cernops/pam_xdg_runtime_dir
* https://github.com/cernops/pam_krb5_cc_move
* https://specifications.freedesktop.org/basedir-spec/basedir-spec-latest.html

