import subprocess
import argparse
from colorama import Fore, Style

def git_push(commit_message):
    try:
        # Ejecutar "git add --all"
        subprocess.run(["git", "add", "--all"], check=True)
        print(Fore.GREEN + ">>> Archivos añadidos al índice.")
        print(Style.RESET_ALL)

        # Ejecutar "git commit -m 'mensaje'"
        subprocess.run(["git", "commit", "-m", commit_message], check=True)
        print(Fore.GREEN + f">>> Commit realizado con mensaje: '{commit_message}'")
        print(Style.RESET_ALL)

        # # Ejecutar "git push"
        # subprocess.run(["git", "push"], check=True)
        # print(Fore.GREEN + ">>> Cambios enviados al repositorio remoto.")
        # print(Style.RESET_ALL)

    except subprocess.CalledProcessError as e:
        print(Fore.RED + f">>> Error al ejecutar el comando: {e}")
        print(Style.RESET_ALL)

    except Exception as e:
        print(Fore.RED + f">>> Error inesperado: {e}")
        print(Style.RESET_ALL)


if __name__ == "__main__":
    # Configurar argparse para recibir el mensaje del commit como argumento
    parser = argparse.ArgumentParser(description=">>> Script para automatizar git add, commit y push.")
    parser.add_argument("mensaje", type=str, help="Mensaje del commit")
    args = parser.parse_args()

    git_push(args.mensaje)