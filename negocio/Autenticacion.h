#ifndef AUTENTICACION_H
#define AUTENTICACION_H

#include <vector>
#include <optional>
#include "../modelos/Usuario.h"
#include "../datos/ArchivoUsuarios.h"

namespace pos {

class Autenticacion {
public:
    explicit Autenticacion(ArchivoUsuarios& archivo);

    bool cargar();
    bool guardar();

    std::optional<Usuario> login(const std::string& username,
                                 const std::string& passwordPlano = "");

    bool verificarUsuarioExiste(const std::string& username);
    
    bool registrarUsuario(const std::string& username,
                         const std::string& passwordPlano,
                         RolUsuario rol,
                         bool requierePassword = true);

    bool agregarUsuario(const Usuario& usuario);

private:
    ArchivoUsuarios& archivo;
    std::vector<Usuario> usuarios;

    std::string hashPassword(const std::string& passwordPlano);
};

} // namespace pos

#endif // AUTENTICACION_H