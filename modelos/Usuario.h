#ifndef USUARIO_H
#define USUARIO_H

#include <string>

namespace pos {

enum class RolUsuario {
    Usuario,
    Administrador
};

struct Usuario {
    std::string username;
    std::string passwordHash;
    RolUsuario rol;
    bool requierePassword = true;  // true for admin, false for standard users
};

} // namespace pos

#endif // USUARIO_H