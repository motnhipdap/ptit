package server.interfaces;

import server.enums.regisStatus;

public interface IRegisService {
    regisStatus regis(String username, String password);

    regisStatus regisWithEmail(String username, String password, String email);
}
