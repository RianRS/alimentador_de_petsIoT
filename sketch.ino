const int servoPin = 2; // Usando o pino 2 como saída
const int feedDuration = 2000; // Duração do fornecimento de alimento em milissegundos
const int buttonPin = 3; // Pino do botão para alternar slots
const int refillButtonPin = 4; // Pino do botão para reabastecer os slots
bool feeding = false;
int foodLevel[4] = {5, 5, 5, 5}; // Níveis de ração para 4 slots
bool slotActive[4] = {true, true, true, true}; // Atividade dos slots (se pode ser aberto ou não)
int currentSlot = 0; // Slot atual que está sendo alimentado
unsigned long lastFeedTime = 0; // Tempo da última alimentação
const unsigned long feedInterval = 180000; // Tempo de intervalo para teste (5 horas)
unsigned long lastSlotChangeTime = 0; // Tempo da última troca de slot
bool buttonPressed = false; // Estado do botão de troca de slot
bool refillButtonPressed = false; // Estado do botão de reabastecimento
unsigned long lastDebounceTime = 0;
const unsigned long debounceDelay = 50; // Debounce de 50ms

void setup() {
    pinMode(servoPin, OUTPUT); // Define o pino do servo como saída
    pinMode(buttonPin, INPUT_PULLUP); // Define o pino do botão de troca de slot como entrada com pull-up
    pinMode(refillButtonPin, INPUT_PULLUP); // Define o pino do botão de reabastecimento como entrada com pull-up
    Serial.begin(115200);
    Serial.println("Simulação do Alimentador de Pets Iniciada");
}

// Função para verificar se todos os slots estão vazios
bool allSlotsEmpty() {
    for (int i = 0; i < 4; i++) {
        if (foodLevel[i] > 0) {
            return false; // Se qualquer slot tiver ração, retorna falso
        }
    }
    return true; // Se todos os slots estiverem vazios, retorna verdadeiro
}

void loop() {
    unsigned long currentMillis = millis();

    // Verifica se o botão de reabastecimento foi pressionado com debounce
    int refillButtonState = digitalRead(refillButtonPin);
    if (refillButtonState == LOW) {
        if ((currentMillis - lastDebounceTime) > debounceDelay && !refillButtonPressed) {
            refillSlots();
            refillButtonPressed = true;
            lastDebounceTime = currentMillis;
        }
    } else {
        refillButtonPressed = false; // Reseta o estado do botão de reabastecimento
    }

    // Verifica se todos os slots estão vazios
    if (!allSlotsEmpty()) {
        // Troca de slot a cada intervalo definido, mas só para slots com ração
        if (currentMillis - lastSlotChangeTime >= feedInterval) {
            changeSlot();
        }

        // Verifica se o botão de troca de slot foi pressionado com debounce
        int buttonState = digitalRead(buttonPin);
        if (buttonState == LOW) {
            if ((currentMillis - lastDebounceTime) > debounceDelay && !buttonPressed) {
                changeSlot();
                buttonPressed = true;
                lastDebounceTime = currentMillis;
            }
        } else {
            buttonPressed = false; // Reseta o estado do botão de troca de slot
        }

        // Simula a alimentação automaticamente a cada intervalo
        if (currentMillis - lastFeedTime >= feedInterval && !feeding) {
            autoFeed(); // Tenta alimentar automaticamente
        }
    } else {
        Serial.println("Todos os slots estão vazios. Reabasteça para continuar.");
    }
}


// Função para reabastecer os slots de ração
void refillSlots() {
    for (int i = 0; i < 4; i++) {
        foodLevel[i] = 5; // Reabastece todos os slots com 5 unidades de ração
        slotActive[i] = true; // Ativa novamente todos os slots
    }
    Serial.println("Todos os slots foram reabastecidos!");
    lastSlotChangeTime = millis(); // Reseta o tempo de troca de slot
}

void changeSlot() {
    int nextSlot;

    // Procura o próximo slot ativo com ração
    do {
        nextSlot = (currentSlot + 1) % 4;
    } while (!slotActive[nextSlot]);

    currentSlot = nextSlot;
    Serial.print("Slot aberto: ");
    Serial.println(currentSlot + 1); // Exibe o slot aberto (1 a 4)
    lastSlotChangeTime = millis(); // Atualiza o tempo da última troca de slot
}

void autoFeed() {
    if (foodLevel[currentSlot] > 0 && !feeding) {
        feeding = true;
        Serial.print("Alimentando o Pet no slot ");
        Serial.println(currentSlot + 1); // Exibe o slot atual

        // Simula a ação do servo motor ligando e desligando o pino
        digitalWrite(servoPin, HIGH); // Simula abrir o dispenser
        delay(feedDuration); // Mantém ligado por 2 segundos
        digitalWrite(servoPin, LOW); // Simula fechar o dispenser

        foodLevel[currentSlot]--; // Reduz o nível de ração do slot atual
        Serial.print("Alimentação Completa! Nível de ração restante no slot ");
        Serial.println(foodLevel[currentSlot]);
    
        // Desativa o slot se a ração acabar
        if (foodLevel[currentSlot] <= 0) {
          slotActive[currentSlot] = false;
          Serial.print("Slot ");
          Serial.print(currentSlot + 1);
          Serial.println(" está sem ração.");
        }

        lastFeedTime = millis(); // Atualiza o tempo da última alimentação
        feeding = false;
      } else if (foodLevel[currentSlot] <= 0) {
          Serial.println("Nível de ração baixo no slot! Reabasteça.");
      }
}