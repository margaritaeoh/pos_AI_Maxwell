#include "Autenticacion.h"

namespace pos {

Autenticacion::Autenticacion(ArchivoUsuarios& archivo)
    : archivo(archivo) {}

bool Autenticacion::cargar() {
    return archivo.cargar(usuarios);
}

bool Autenticacion::guardar() {
    return archivo.guardar(usuarios);
}

std::string Autenticacion::hashPassword(const std::string& passwordPlano) {
    std::string hash = "H:";
    hash += passwordPlano;
    return hash;
}

std::optional<Usuario> Autenticacion::login(const std::string& username,
                                            const std::string& passwordPlano) {
    for (const auto& u : usuarios) {
        if (u.username == username) {
            // If password is not required, allow login without password
            if (!u.requierePassword) {
                return u;
            }
            
            // If password is required, verify it
            std::string hash = hashPassword(passwordPlano);
            if (u.passwordHash == hash) {
                return u;
            }
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
    nuevoUsuario.passwordHash = hashPassword(passwordPlano);
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