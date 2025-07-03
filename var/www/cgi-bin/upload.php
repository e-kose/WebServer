<?php
// upload.php - Güncellenmiş versiyon
header('Content-Type: application/json');

// Hata raporlamayı aç
ini_set('display_errors', 1);
ini_set('display_startup_errors', 1);
error_reporting(E_ALL);

$UPLOAD_DIR = realpath(__DIR__ . '/../uploads');
$MAX_SIZE = 100 * 1024 * 1024; // 100MB
$LOG_FILE = '/tmp/webserver_upload.log';

// Hata ayıklama için gelen verileri logla
file_put_contents($LOG_FILE, "Received content type: " . $_SERVER['CONTENT_TYPE'] . "\n", FILE_APPEND);
file_put_contents($LOG_FILE, "Request method: " . $_SERVER['REQUEST_METHOD'] . "\n", FILE_APPEND);

// POST kontrolü
if ($_SERVER['REQUEST_METHOD'] !== 'POST') {
    $response = ["status" => "error", "message" => "Only POST requests are accepted"];
    echo json_encode($response);
    exit;
}

// Çıktıyı doğrudan gönder
if (isset($_FILES['file'])) {
    // Dosya işleme kodu burada
} else {
    $input = file_get_contents('php://input');
    file_put_contents($LOG_FILE, "Raw input (" . strlen($input) . " bytes):\n$input\n", FILE_APPEND);
    
    $response = [
        "status" => "error",
        "message" => "No file received",
        "received_data" => base64_encode($input) // Debug için
    ];
    
    echo json_encode($response);
    exit;
}
?>