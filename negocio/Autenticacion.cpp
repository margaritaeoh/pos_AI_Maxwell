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
    std::string hash = hashPassword(passwordPlano);
    for (const auto& u : usuarios) {
        if (u.username == username && u.passwordHash == hash) {
            return u;
        }
    }
    return std::nullopt;
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