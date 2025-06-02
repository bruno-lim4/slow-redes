ssize_t UDPSocket::send(const std::vector<char>& data) {
    if (!isConnected_) {
        lastErrno_ = ENOTCONN;
        return -1;
    }

    // Chama sendto() enviando data.data(), data.size()
    ssize_t sent = ::sendto(
        sockfd_,
        data.data(),
        data.size(),
        0,
        reinterpret_cast<const struct sockaddr*>(&peerAddr_),
        sizeof(peerAddr_)
    );
    if (sent < 0) {
        lastErrno_ = errno;
    }
    return sent;
}

std::optional<std::vector<char>> UDPSocket::receive() {
    if (sockfd_ < 0) {
        lastErrno_ = EBADF;
        return std::nullopt;
    }

    // Buffer temporário (UDP cabe, no máximo, ~65507 bytes de payload)
    char buffer[65536];
    ssize_t recvd = ::recvfrom(
        sockfd_,
        buffer,
        sizeof(buffer),
        0,
        nullptr,
        nullptr
    );
    if (recvd < 0) {
        lastErrno_ = errno;
        return std::nullopt; // timeout ou erro
    }

    // Copia para um std::vector<char> do tamanho exato
    std::vector<char> result(static_cast<size_t>(recvd));
    std::memcpy(result.data(), buffer, static_cast<size_t>(recvd));
    return result;
}

int UDPSocket::getLastError() const {
    return lastErrno_;
}
