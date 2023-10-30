int main() {
    return 0;
}

int accessCheck(unsigned int uid, unsigned int gid, unsigned int p, int f) {
    Permission permission = getPermission(f);

    if (uid == permission.uid) {
        return p | permission.u == p ? 1 : 0;
    } else if (gid == permission.gid) {
        return p | permission.g == p ? 1 : 0;
    }

    return p | permission.o == p ? 1 : 0;
}