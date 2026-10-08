"""Narrow transient EINVAL retry for exact saved WinMain bytes; failures remain recorded."""
import hashlib,time

def restore_saved(path,saved,attempts,sleep=time.sleep):
    # Initial attempt plus at most three 250-ms retries, only for errno22.
    for attempt in range(1,5):
        row={'attempt':attempt,'path':str(path),'expected_sha256':hashlib.sha256(saved).hexdigest()}
        attempts.append(row)
        try:
            path.write_bytes(saved)
            actual=path.read_bytes()
            row['actual_sha256']=hashlib.sha256(actual).hexdigest()
            row['byte_equal']=actual==saved
            if actual!=saved:
                row['status']='verification_failed'
                raise RuntimeError('WinMain restoration byte verification failed')
            row['status']='restored'
            return
        except OSError as error:
            row.update(status='write_or_read_failed',errno=error.errno,error=str(error))
            if error.errno!=22 or attempt==4:
                raise
            row['retry_delay_seconds']=.25
            sleep(.25)
