# GPU Switcher for Dell XPS 15 9570

## How to use

- Do everything EXCEPT CREATING MANAGE SCRIPTS on [this part of Arch Linux Wiki page](https://wiki.archlinux.org/index.php/Dell_XPS_15_9570#Manually_loading/unloading_NVIDIA_module).

- Install `cmake` and `gcc`

  ``` shell
  sudo pacman -S cmake gcc
  ```

- Clone this repository.

- Modify `user.conf` 

  ``` config
  userconfig	$HOME/.config/GpuSwitcher/user.conf
  default	off
  ```

  **$HOME SHOULD BE THE FULL PATH FOR YOUR CURRENT USER OR THE USER YOU WANT TO MANAGE THE GPU**  

  This will be automatically done in the future version. 

- Build and Install

  ``` shell
  mkdir build && cd build
  cmeke ..
  make -j12
  sudo make install
  ```

- Enable Services

  ``` shell
  sudo systemctl enable gpud.service
  ```

- Turn on or turn off GPU manually

  ``` shell
  #Turn on
  gpum on
  #Turn off
  gpum off
  ```

- OR add `gpum on` and `gpum off` to your power management scripts when connecting or disconnecting power supply.

  On KDE Plasma, it is quite easy.

## Benefits compared to manually way or automatic way in the Wiki  

### Manually way

- No root privilege needed.
- Can be a part of automatic scripts. 

### Automatic way

- Easy

- No need to install [bumblebee-git](https://aur.archlinux.org/packages/bumblebee-git/)or [bumblebee-forceunload](https://aur.archlinux.org/packages/bumblebee-forceunload/) which requires some time to compile and manually upgrades every time bumblebee updates.

  