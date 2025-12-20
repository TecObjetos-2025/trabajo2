#pragma once

#include <memory>
#include <string>

class Cliente;

namespace repos
{

    class ClienteRepository
    {
    public:
        ClienteRepository();
        ~ClienteRepository();

        // Busca un cliente por nombre en la base de datos. Devuelve nullptr si no existe
        std::shared_ptr<Cliente> findByName(const std::string &name);

        // Crea un cliente a partir de los datos y lo persiste en DB
        std::shared_ptr<Cliente> create(const std::string &name, const std::string &taxId);
    };

} // namespace repos
