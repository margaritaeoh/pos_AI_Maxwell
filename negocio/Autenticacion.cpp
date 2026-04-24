#include "Autenticacion.h"
#include "PasswordHasher.h"

namespace pos {

Autenticacion::Autenticacion(ArchivoUsuarios& archivo)
    : archivo(archivo) {}

bool Autenticacion::cargar() {
    return archivo.cargar(usuarios);
}

bool Autenticacion::guardar() {
    return archivo.guardar(usuarios);
}

std::optional<Usuario> Autenticacion::login(const std::string& username,
                                            const std::string& passwordPlano) {
    for (auto& user : usuarios) {
        if (user.username == username) {
            // If password is not required, allow login without password
            if (!user.requierePassword) {
                return user;
            }

            // Verify password (supports legacy "H:" and current "H2:" formats)
            if (!PasswordHasher::verifyPassword(username, passwordPlano, user.passwordHash)) {
                continue;
            }

            // Upgrade legacy hash to SHA-256 on successful login
            if (PasswordHasher::needsUpgrade(user.passwordHash)) {
                user.passwordHash = PasswordHasher::hashPassword(username, passwordPlano);
                archivo.guardar(usuarios);
            }

            return user;
        }
    }
    return std::nullopt;
}

bool Autenticacion::verificarUsuarioExiste(const std::string& username) {
    for (const auto& u : usuarios) {
        if (u.username == username) {
            return true;
        }
    }
    return false;
}

bool Autenticacion::registrarUsuario(const std::string& username,
                                     const std::string& passwordPlano,
                                     RolUsuario rol,
                                     bool requierePassword) {
    if (verificarUsuarioExiste(username)) {
        return false;
    }

    Usuario nuevoUsuario;
    nuevoUsuario.username = username;
    nuevoUsuario.passwordHash = PasswordHasher::hashPassword(username, passwordPlano);
    nuevoUsuario.rol = rol;
    nuevoUsuario.requierePassword = requierePassword;

    usuarios.push_back(nuevoUsuario);
    return true;
}

bool Autenticacion::agregarUsuario(const Usuario& usuario) {
    for (const auto& u : usuarios) {
        if (u.username == usuario.username) {
            return false;
        }
    }
    usuarios.push_back(usuario);
    return true;
}

} // namespace pos