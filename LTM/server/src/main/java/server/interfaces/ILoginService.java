package server.interfaces;

import server.utils.LoginResult;

public interface ILoginService {
    LoginResult login(String username, String password);
}
