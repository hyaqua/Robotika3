#include <Servo.h>
#include <EEPROM.h>

// C++ code
//

const int left_photores = A4;
const int right_photores = A5;
int left_res = 0;
int right_res = 0;
bool read_photoresistors = false;

const int btn_pin = 2;
unsigned long last_int = 0;
unsigned long debounce = 200;
bool btn_pressed = false;

const int sens_vals[] = {512,256,128,64,32};
int sens = 0;

const int light_pins[] = {5,6};
int light_vals[] = {0,0};


const int servo_pin = 9;
Servo servo;
int servo_val = 90;
int amount = 1;
bool move_servo = true;

int save = 0; // Autosave counter

void setup() {
  cli(); // stop interrupts
  TCCR2A = 0; // set entire TCCR2A register to 0
  TCCR2B = 0; // same for TCCR2B
  TCNT2  = 0; // initialize counter value to 0
  // set compare match register for 1000 Hz increments
  OCR2A = 249; // = 16000000 / (64 * 1000) - 1 (must be <256)
  // turn on CTC mode
  TCCR2B |= (1 << WGM21);
  // Set CS22, CS21 and CS20 bits for 64 prescaler
  TCCR2B |= (1 << CS22) | (0 << CS21) | (0 << CS20);
  // enable timer compare interrupt
  TIMSK2 |= (1 << OCIE2A);
  sei(); // allow interrupts


  Serial.begin(9600);  // for printing current status;

  attachInterrupt(digitalPinToInterrupt(btn_pin), btn_interrupt, RISING);
  pinMode(servo_pin, OUTPUT);
  pinMode(light_pins[0], OUTPUT);
  pinMode(light_pins[1], OUTPUT);
  pinMode(light_pins[2], OUTPUT);
  
  servo.attach(servo_pin);
  
  get_sens();
}


ISR(TIMER2_COMPA_vect){ // Every ~1 ms
  read_photoresistors = true;
  save++;
}
void debug(){
  Serial.print("left_res: ");
  Serial.print(left_res);
  Serial.print("\t\tright_res: ");
  Serial.print(right_res);
  Serial.print("\t\tsens_val: ");
  Serial.print(sens_vals[sens]);
  Serial.print("\t\tservo_val: ");
  Serial.println(servo_val);
}
void loop() {
  if(btn_pressed){
  	set_sensitivity();
    btn_pressed = false;
  }
  if(move_servo){
    servo.write(servo_val);
    move_servo = false;
  }
  if(read_photoresistors){
  	read_photores();
  }
  
  if(save > 1000){ // Autosave sensitivity
    save_sens();
    save = 0;
  }
  
  debug();
  
  if(read_photoresistors && abs(left_res - right_res) > sens_vals[sens]){
    
    if(left_res > right_res){
      servo_val -= amount;
    }
    if(left_res < right_res){
      servo_val += amount;
    }
	  servo_val = constrain(servo_val, 0, 160);
    move_servo = true;
    read_photoresistors = false;
  }
}

void set_sensitivity(){ // Change sens value and lights
  sens++;
  if(sens > 4) sens = 0;
  light_vals[0] = map(sens,0,4, 0, 1023);
  light_vals[0] = constrain(light_vals[0],0,1023);
  light_vals[1] = map(sens,1,4, 0, 1023);
  light_vals[1] = constrain(light_vals[1],0,1023);
  analogWrite(light_pins[0], light_vals[0]);
  analogWrite(light_pins[1], light_vals[1]);


}

void read_photores(){
  left_res = analogRead(left_photores);
  right_res = analogRead(right_photores);
}

bool check_crc() {
  unsigned int saved_crc;
  EEPROM.get(sizeof(int)*1, saved_crc); // we retrieve what we currently have in crc
  unsigned int calculated_crc = get_eeprom_crc(0, sizeof(int)*1); // Check first 2 bytes because they contain the sens

  return (saved_crc == calculated_crc);
}

void set_crc(){
  unsigned int calculated_crc = get_eeprom_crc(0, sizeof(int)*1);
  unsigned int saved_crc;
  EEPROM.get(sizeof(int)*1, saved_crc);
  if(saved_crc != calculated_crc){
    EEPROM.put(0,calculated_crc);
  }
}

unsigned int get_eeprom_crc(int offset, size_t data_len) {
    unsigned int crc = 0;
    
    static const unsigned int crc_table[16] = {
    0x0000, 0xcc01, 0xd801, 0x1400, 0xf001, 0x3c00, 0x2800, 0xe401,
    0xa001, 0x6c00, 0x7800, 0xb401, 0x5000, 0x9c01, 0x8801, 0x4400
    };

    data_len = data_len + offset;

    for (int idx = offset; idx < data_len; ++idx){
      crc = crc_table[(crc ^ EEPROM[idx]) & 0x0f] ^ (crc >> 4);
      crc = crc_table[(crc ^ (EEPROM[idx] >> 4)) & 0x0f] ^ (crc >> 4);
    }


    return crc & 0xffff;
}

void get_sens() {
  if(check_crc()){
  EEPROM.get(0, sens);
  }
  else{
    sens = 0;
  }
}
void save_sens() {
  int temp;
  EEPROM.get(0,temp);
  if(temp == sens) return;
  EEPROM.put(0, sens);
  set_crc();
}

void btn_interrupt(){
  unsigned long curr_time = millis();
  if(curr_time - last_int > debounce){
  	btn_pressed = true;
  }
  last_int = curr_time;
}