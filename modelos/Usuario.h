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
};

} // namespace pos

#endif // USUARIO_H